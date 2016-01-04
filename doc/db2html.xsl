<?xml version="1.0" encoding="UTF-8"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2016 Martin Gieseking -->
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:exsl="http://exslt.org/common"
	xmlns:func="http://exslt.org/functions"
	xmlns:my="my-namespace"
	extension-element-prefixes="exsl func">

	<xsl:output method="html"/>

	<xsl:template match="/">
		<html>
			<head>
				<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=UTF-8"/>
				<link rel="stylesheet" href="dvisvgm.css"/>
				<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.2/css/bootstrap.min.css"/>
				<link rel="stylesheet" href="//maxcdn.bootstrapcdn.com/font-awesome/4.3.0/css/font-awesome.min.css"/>
			</head>
			<body>
				<xsl:apply-templates/>
				<hr/>
				<div class="row">
					<div class="col-md-6">
						<xsl:text>Version </xsl:text>
						<xsl:value-of select="*/refmeta/refmiscinfo[@class='version']"/>
					</div>
					<div class="col-md-6">
						<span class="pull-right" style="padding-bottom:1em">
							<xsl:text>Last updated: </xsl:text>
							<xsl:value-of select="substring-before(*/refmeta/refmiscinfo[@class='date'], ' ')"/>
						</span>
					</div>
				</div>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="refnamediv">
		<section id="_name">
			<h2 style="display:inline"><i class="fa fa-tag fa-fw"/> Name</h2>
			<div class="rfloat frame">
				<a href="{refname}.pdf"><i class="fa fa-file-pdf-o"/> PDF version</a>
			</div>
			<div class="sectionbody">
				<xsl:value-of select="concat(refname, ' &#8211; ', refpurpose)"/>
			</div>
		</section>
	</xsl:template>

	<xsl:template match="refsynopsisdiv">
		<xsl:if test="@id">
			<span class="linktarget" id="{@id}"/>
		</xsl:if>
		<section>
			<h2><i class="fa fa-terminal fa-fw"/> Synopsis</h2>
			<div class="sectionbody">
				<xsl:apply-templates/>
			</div>
		</section>
	</xsl:template>

	<xsl:template match="refsect1">
		<xsl:if test="@id">
			<span class="linktarget" id="{@id}"/>
		</xsl:if>
		<section>
			<h2>
				<xsl:copy-of select="my:fix-section-title(title)"/>
			</h2>
			<div class="sectionbody">
				<xsl:apply-templates/>
			</div>
		</section>
	</xsl:template>

	<xsl:template match="variablelist">
		<dl>
			<xsl:apply-templates/>
		</dl>
	</xsl:template>

	<xsl:template match="varlistentry">
		<dt class="hdlist1">
			<xsl:apply-templates/>
		</dt>
	</xsl:template>

	<xsl:template match="varlistentry/listitem">
		<dd><xsl:apply-templates></xsl:apply-templates></dd>
	</xsl:template>

	<xsl:template match="programlisting|screen">
		<pre><code><xsl:apply-templates/></code></pre>
	</xsl:template>

	<xsl:template match="caution|note">
		<div class="admonitionblock">
			<table>
				<tr>
					<td class="icon">
						<img src="./{local-name()}.png" alt="{local-name()}"/>
					</td>
					<td class="content">
						<xsl:apply-templates/>
					</td>
				</tr>
			</table>
		</div>
	</xsl:template>

	<xsl:template match="informaltable">
		<xsl:if test="@id">
			<span class="linktarget" id="{@id}"/>
		</xsl:if>
		<table>
			<xsl:apply-templates/>
		</table>
	</xsl:template>

	<xsl:template match="row">
		<tr>
			<xsl:apply-templates/>
		</tr>
	</xsl:template>

	<xsl:template match="row/entry">
		<td style="padding-right:10px">
			<xsl:apply-templates/>
		</td>
	</xsl:template>

	<xsl:template match="ulink">
		<a href="{@url}"><xsl:apply-templates/></a>
	</xsl:template>

	 <xsl:template match="link[@linkend]">
		 <a href="#{@linkend}"><xsl:apply-templates/></a>
	 </xsl:template>

	<xsl:template match="emphasis[@role='strong']">
		<strong><xsl:apply-templates/></strong>
	</xsl:template>

	<xsl:template match="emphasis">
		<em><xsl:apply-templates/></em>
	</xsl:template>

	<xsl:template match="literal">
		<code><xsl:apply-templates/></code>
	</xsl:template>

	<xsl:template match="simpara">
		<xsl:if test="@id">
			<span class="linktarget" id="{@id}"/>
		</xsl:if>
		<p><xsl:apply-templates/></p>
	</xsl:template>

	<xsl:template match="title|refentryinfo|refmeta"/>

	<func:function name="my:fix-section-title">
		<xsl:param name="name"/>
		<xsl:variable name="sections">
			<section icon="info-circle">Description</section>
			<section icon="sliders">Options</section>
			<section icon="plug">Supported Specials</section>
			<section icon="keyboard-o">Examples</section>
			<section icon="globe">Environment</section>
			<section icon="files-o">Files</section>
			<section icon="mail-forward">See also</section>
			<section icon="link">Resources</section>
			<section icon="bug">Bugs</section>
			<section icon="laptop">Author</section>
			<section icon="gavel">Copying</section>
		</xsl:variable>
		<xsl:variable name="mixed-name" select="exsl:node-set($sections)/section[my:toupper(.)=$name]"/>
		<func:result>
			<xsl:choose>
				<xsl:when test="$mixed-name">
					<xsl:if test="$mixed-name/@icon">
						<i class="fa fa-{$mixed-name/@icon} fa-fw"/>
					</xsl:if>
					<xsl:value-of select="concat(' ', $mixed-name)"/>
				</xsl:when>
				<xsl:otherwise>
					<value-of select="$name"/>
				</xsl:otherwise>
			</xsl:choose>
		</func:result>
	</func:function>

	<func:function name="my:toupper">
		<xsl:param name="str"/>
		<func:result select="translate($str, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
	</func:function>
</xsl:stylesheet>
